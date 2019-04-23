package com.amazon.ask.Quiny.AmazonHandlers;

import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.model.Response;

import java.util.Optional;

public class SessionEndedHandler implements com.amazon.ask.dispatcher.request.handler.RequestHandler {
    @Override
    public boolean canHandle(HandlerInput input) {
        return false;
    }

    @Override
    public Optional<Response> handle(HandlerInput input) {
        return Optional.empty();
    }
}
