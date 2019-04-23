package com.amazon.ask.Quiny.handlers;

import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.model.Response;
import com.amazon.ask.request.Predicates;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;
import com.amazon.ask.Quiny.utils.QuinyUtils;

import java.util.Map;
import java.util.Optional;

public class YesNoIntentHandler implements com.amazon.ask.dispatcher.request.handler.RequestHandler {
    @Override
    public boolean canHandle(HandlerInput input) {
        return input
                .matches(Predicates.intentName("AMAZON.YesIntent")
                        .or(Predicates.intentName("AMAZON.NoIntent")));
    }

    @Override
    public Optional<Response> handle(HandlerInput input) {
        Map<String, Object> sessionAttributes = input.getAttributesManager().getSessionAttributes();
        if ((boolean) sessionAttributes.get(Attributes.RETRY)) {
            if (input.matches(Predicates.intentName("AMAZON.YesIntent"))) return QuinyUtils.listenOptions(input);
        }

        return input.getResponseBuilder()
                .withShouldEndSession(true)
                .withSpeech(Constants.INTENSE_TALK[0] + "eeem" + Constants.INTENSE_TALK[1] +
                        ",De acuerdo, cuando gustes puedes volver a llamarme." + Constants.INTENSE_TALK[0] +
                        "hasta luego" + Constants.INTENSE_TALK[1])
                .build();
    }
}
