#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Specifies a line break conversion mode.
typedef enum UriBreakConversionEnum {
	URI_BR_TO_LF, /**< Convert to Unix line breaks ("\\x0a") */
	URI_BR_TO_CRLF, /**< Convert to Windows line breaks ("\\x0d\\x0a") */
	URI_BR_TO_CR, /**< Convert to Macintosh line breaks ("\\x0d") */
	URI_BR_TO_UNIX = URI_BR_TO_LF, /**< @copydoc UriBreakConversionEnum::URI_BR_TO_LF */
	URI_BR_TO_WINDOWS = URI_BR_TO_CRLF, /**< @copydoc UriBreakConversionEnum::URI_BR_TO_CRLF */
	URI_BR_TO_MAC = URI_BR_TO_CR, /**< @copydoc UriBreakConversionEnum::URI_BR_TO_CR */
	URI_BR_DONT_TOUCH /**< Copy line breaks unmodified */
} UriBreakConversion; /**< @copydoc UriBreakConversionEnum */

typedef struct QueryValue {

    char *key;
    char *value;

    struct QueryValue *next;

} QueryValue;

static int uri_dissect_query (QueryValue** dest, int * itemCount,
		const char *first, const char *afterLast,
		int plusToSpace, UriBreakConversion breakConversion,
		UriMemoryManager * memory) {

	const char *walk = first;
	const char *keyFirst = first;
	const char *keyAfter = NULL;
	const char *valueFirst = NULL;
	const char *valueAfter = NULL;

	URI_TYPE(QueryList) ** prevNext = dest;
	int nullCounter;
	int * itemsAppended = (itemCount == NULL) ? &nullCounter : itemCount;

	if ((dest == NULL) || (first == NULL) || (afterLast == NULL)) {
		return 1;
	}

	if (first > afterLast) {
		return 1;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	*dest = NULL;
	*itemsAppended = 0;

	/* Parse query string */
	for (; walk < afterLast; walk++) {
		switch (*walk) {
		case '&':
			if (valueFirst != NULL) {
				valueAfter = walk;
			} else {
				keyAfter = walk;
			}

			if (URI_FUNC(AppendQueryItem)(prevNext, itemsAppended,
					keyFirst, keyAfter, valueFirst, valueAfter,
					plusToSpace, breakConversion, memory)
					== URI_FALSE) {
				/* Free list we built */
				*itemsAppended = 0;
				URI_FUNC(FreeQueryListMm)(*dest, memory);
				return URI_ERROR_MALLOC;
			}

			/* Make future items children of the current */
			if ((prevNext != NULL) && (*prevNext != NULL)) {
				prevNext = &((*prevNext)->next);
			}

			if (walk + 1 < afterLast) {
				keyFirst = walk + 1;
			} else {
				keyFirst = NULL;
			}
			keyAfter = NULL;
			valueFirst = NULL;
			valueAfter = NULL;
			break;

		case '=':
			/* NOTE: WE treat the first '=' as a separator, */
			/*       all following go into the value part   */
			if (keyAfter == NULL) {
				keyAfter = walk;
				if (walk + 1 <= afterLast) {
					valueFirst = walk + 1;
					valueAfter = walk + 1;
				}
			}
			break;

		default:
			break;
		}
	}

	if (valueFirst != NULL) {
		/* Must be key/value pair */
		valueAfter = walk;
	} else {
		/* Must be key only */
		keyAfter = walk;
	}

	if (URI_FUNC(AppendQueryItem)(prevNext, itemsAppended, keyFirst, keyAfter,
			valueFirst, valueAfter, plusToSpace, breakConversion, memory)
			== URI_FALSE) {
		/* Free list we built */
		*itemsAppended = 0;
		URI_FUNC(FreeQueryListMm)(*dest, memory);
		return URI_ERROR_MALLOC;
	}

	return 0;
}