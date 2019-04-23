package com.amazon.ask.Quiny.handlers;

import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.model.Response;
import com.amazon.ask.request.Predicates;
import com.amazon.ask.Quiny.utils.QuinyUtils;

import java.util.Optional;

public class ListenOptionsIntentHandler implements com.amazon.ask.dispatcher.request.handler.RequestHandler {

    @Override
    public boolean canHandle(HandlerInput input) {
        return input.matches(Predicates.intentName("ListenOptionsIntent")
                .and(Predicates.sessionAttribute(Attributes.GAME_SELECTED,-1)));
    }

    @Override
    public Optional<Response> handle(HandlerInput input) {
        return QuinyUtils.listenOptions(input);
    }
}
