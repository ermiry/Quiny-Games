package com.amazon.ask.Quiny.AmazonHandlers;

import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.dispatcher.request.handler.RequestHandler;
import com.amazon.ask.model.LaunchRequest;
import com.amazon.ask.model.Response;
import com.amazon.ask.request.Predicates;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;

import java.util.Map;
import java.util.Optional;
import static com.amazon.ask.request.Predicates.requestType;


public class LaunchRequestHandler implements RequestHandler {
    @Override
    public boolean canHandle(HandlerInput input) {
        return input.matches(requestType(LaunchRequest.class));
    }

    @Override
    public Optional<Response> handle(HandlerInput input) {
//        String accessToken;
//        System.out.println("Start");
//        if((accessToken = input.getRequestEnvelope().getContext().getSystem().getUser().getAccessToken())==null){
//            return input.getResponseBuilder()
//                    .withSpeech("No estas registrado aun, por favor abre la app de alexa para registrarte")
//                    .withReprompt(Constants.HELP_MESSAGE)
//                    .withShouldEndSession(true)
//                    .withLinkAccountCard()
//                    .build();
//        }
//
//        //Say a code to register the current session with the page
//        Map<String,Object> sessionAttributes = input.getAttributesManager().getSessionAttributes();
//        //sessionAttributes.put(Attributes.STATE,Attributes.START_STATE);
//        sessionAttributes.put(Attributes.ACCESS_TOKEN,accessToken);

        return input.getResponseBuilder()
                .withSpeech(Constants.WELCOME_MESSAGE + ". " + Constants.INFO_MESSAGE)
                .withReprompt(Constants.HELP_MESSAGE)
                .withShouldEndSession(false)
                .build();
    }
}
