package com.amazon.ask.Quiny.handlers;

import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.model.IntentRequest;
import com.amazon.ask.model.Response;
import com.amazon.ask.model.Slot;
import com.amazon.ask.request.Predicates;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;

import java.io.IOException;
import java.util.Map;
import java.util.Optional;

public class OptionIntentHandler implements com.amazon.ask.dispatcher.request.handler.RequestHandler {
    @Override
    public boolean canHandle(HandlerInput input) {
        return input
                .matches(Predicates.intentName("OptionIntent")
                        .and(Predicates.sessionAttribute(Attributes.RETRY, true)));
    }

    @Override
    public Optional<Response> handle(HandlerInput input) {
        Map<String, Object> sessionAttributes = input.getAttributesManager().getSessionAttributes();
        String accessToken = "hola";
        String responseText;

        try {
            if (accessToken == null) {
                throw new IOException();
            }
            try {
                IntentRequest intentRequest = (IntentRequest) input.getRequestEnvelope().getRequest();
                Map<String, Slot> slots = intentRequest.getIntent().getSlots();
                String option = slots.get("Option").getValue();
                switch (option) {
                    case "quien sabe mas":
                    case "Quien sabe más":

                    case "quien sabe más":
                    case "QUIEN SABE MAS":
                    case "QUIEN SABE MÁS":
                        sessionAttributes.put(Attributes.GAME_SELECTED, Constants.QUIEN_SABE_MAS);
                        sessionAttributes.put(Attributes.STATE, Attributes.WAITING);
                        //sessionAttributes.put(Attributes.RETRY,true);
                        responseText = Constants.INTENSE_TALK[0] + "bien" + Constants.INTENSE_TALK[1] + ", Se eligio la opción Quién sabe más, ¿Estás Listo?";

                        break;
                    case "Conoceton":
                    case "conoceton":
                    case "CONOCETON":
                        sessionAttributes.put(Attributes.GAME_SELECTED, Constants.CONOCETON);
                        sessionAttributes.put(Attributes.STATE, Attributes.WAITING);
                        responseText = Constants.INTENSE_TALK[0] + "bravo" + Constants.INTENSE_TALK[1] + ", Se eligio la opción Conocetón ¿Estás Listo?";

                        break;
                    case "Mateton":
                    case "MATETON":
                    case "mateton":
                        sessionAttributes.put(Attributes.GAME_SELECTED, Constants.MATETON);
                        sessionAttributes.put(Attributes.STATE, Attributes.WAITING);
                        responseText = Constants.INTENSE_TALK[0] + "así mero" + Constants.INTENSE_TALK[1] + ",Se eligio la opción Matetón ¿Estás Listo?";
                        break;
                    default:
                        throw new NullPointerException();

                }
                sessionAttributes.put(Attributes.RETRY, false);
            } catch (NullPointerException e) {
                System.out.println("Option is invalid or not exist");
                responseText = Constants.INTENSE_TALK[0] + "buuuh" + Constants.INTENSE_TALK[1];
                responseText += "¿No se ha detectado tu opcion, gustas reintentar?";
                sessionAttributes.put(Attributes.RETRY, true);
            }
        } catch (IOException e) {
            responseText = "No estas conectado, por favor ve a alexa app para conectarte";
            return input.getResponseBuilder()
                    .withShouldEndSession(true)
                    .withSpeech(responseText)
                    .withLinkAccountCard()
                    .build();
        }

        return input.getResponseBuilder()
                .withSpeech(responseText)
                .withReprompt(Constants.HELP_MESSAGE)
                .withShouldEndSession(false)
                .build();
    }
}
