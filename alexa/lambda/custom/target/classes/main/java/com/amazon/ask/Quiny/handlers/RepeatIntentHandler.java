package com.amazon.ask.Quiny.handlers;


import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.dispatcher.request.handler.RequestHandler;
import com.amazon.ask.model.Response;
import com.amazon.ask.request.Predicates;
import com.amazon.ask.Quiny.model.Attributes;

import java.util.Optional;

public class RepeatIntentHandler implements RequestHandler {
    @Override
    public boolean canHandle(HandlerInput input) {
        return input.matches(Predicates.intentName("AMAZON.RepeatIntent")
                .and(Predicates.sessionAttribute(Attributes.STATE, Attributes.QUIZ_STATE)));
    }

    @Override
    public Optional<Response> handle(HandlerInput input) {
        String speech = "Waiting for tasks";
        return input.getResponseBuilder()
                .withShouldEndSession(false)
                .withSpeech(speech)
                .build();
    }
}
