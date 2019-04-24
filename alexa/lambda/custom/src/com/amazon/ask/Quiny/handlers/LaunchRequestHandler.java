package com.amazon.ask.Quiny.handlers;

import com.amazon.ask.Quiny.Api.FunctionApi;
import com.amazon.ask.Quiny.utils.QuinyUtils;
import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.dispatcher.request.handler.RequestHandler;
import com.amazon.ask.model.LaunchRequest;
import com.amazon.ask.model.Response;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;
import com.amazon.ask.model.interfaces.alexa.presentation.apl.RenderDocumentDirective;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;


import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.HashMap;
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
        String accessToken;
        Map<String, Object> document = null;
        try {
            ObjectMapper mapper = new ObjectMapper();
            TypeReference<HashMap<String, Object>> documentMapType = new TypeReference<HashMap<String, Object>>() {
            };
            JsonObject doc = new JsonParser()
                    .parse(FunctionApi.getInstance()
                            .sendGet(FunctionApi.getInstance().UNIVERSAL_URL + "/views/apl",new HashMap<>())).getAsJsonObject();
            System.out.println(doc);
            document = mapper.readValue(doc.toString() , documentMapType);

        }catch(IOException | NullPointerException e){
            e.printStackTrace();
        }
        System.out.println("Start");
        if ((accessToken = input.getRequestEnvelope().getContext().getSystem().getUser().getAccessToken()) == null) {
            return input.getResponseBuilder()
                    .withSpeech("No estas registrado aun, por favor abre la app de alexa para registrarte")
                    .withReprompt(Constants.HELP_MESSAGE)
                    .withShouldEndSession(true)
                    .withLinkAccountCard()

                    .build();
        }
        //Say a code to register the current session with the page
        Map<String, Object> sessionAttributes = input.getAttributesManager().getSessionAttributes();
        sessionAttributes.put(Attributes.ACCESS_TOKEN,accessToken);
        sessionAttributes.put(Attributes.STATE, Attributes.START_STATE);
        sessionAttributes.put(Attributes.RETRY, true);
        sessionAttributes.put(Attributes.GAME_SELECTED,-1);

        if(QuinyUtils.supportsApl(input))
            return input.getResponseBuilder()
                .withSpeech("<audio src='https://s3.amazonaws.com/audioquiny/audio/start.mp3'/> "  + Constants.WELCOME_MESSAGE + ". " + Constants.INFO_MESSAGE)
                .addDirective(RenderDocumentDirective.builder()
                        .withDocument(document).build())
                .withReprompt(Constants.HELP_MESSAGE)

                .withShouldEndSession(false)
                .build();
        else
            return input.getResponseBuilder()
            .withSpeech("<audio src='https://s3.amazonaws.com/audioquiny/audio/start.mp3'/> "  + Constants.WELCOME_MESSAGE + ". " + Constants.INFO_MESSAGE)
            .withReprompt(Constants.HELP_MESSAGE)
            .withShouldEndSession(false)
            .build();
    }
}
