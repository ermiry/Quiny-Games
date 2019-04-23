package com.amazon.ask.Quiny.handlers;

import com.amazon.ask.Quiny.model.Parameters;
import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.model.Response;
import com.amazon.ask.request.Predicates;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import com.amazon.ask.Quiny.Api.FunctionApi;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;
import com.amazon.ask.Quiny.utils.QuinyUtils;

import java.io.BufferedReader;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class QuienSabeMasStarterIntentHandler implements com.amazon.ask.dispatcher.request.handler.RequestHandler {
    @Override
    public boolean canHandle(HandlerInput input) {
        return input
                .matches(Predicates.sessionAttribute(Attributes.STATE, Attributes.WAITING)
                        .and(Predicates.sessionAttribute(Attributes.GAME_SELECTED, Constants.QUIEN_SABE_MAS))
                        .and(Predicates.intentName("AMAZON.YesIntent")));
    }

    //Bienvenidos
    //Session Started
    @Override
    public Optional<Response> handle(HandlerInput input) {
        Map<String, Object> sessionAttributes = input.getAttributesManager().getSessionAttributes();
        Map<String, String> params = new HashMap<>();
        String accessToken = QuinyUtils.getAccessToken(input);
        sessionAttributes.put(Attributes.STATE, Attributes.QUIZ_STATE);
        String responseText;
        int numberOfParticipants = 0;
        BufferedReader in;
        try {
            if (accessToken == null) throw new Exception();
            params.put(Constants.ACCESS_TOKEN, accessToken);
            params.put("type", String.valueOf(Constants.QUIEN_SABE_MAS));

            in = FunctionApi.getInstance()
                    .sendGet(FunctionApi.getInstance().UNIVERSAL_URL + "/start", params);

            JsonObject response = new JsonParser().parse(in).getAsJsonObject();

            numberOfParticipants = response.get("participants").getAsInt();

            String sessionID = response.get("sessionID").getAsString();

            String lobbyID = response.get("lobbyID").getAsString();

            sessionAttributes.put(Attributes.SESSION_ID, sessionID);

            sessionAttributes.put(Attributes.LOBBY_ID, lobbyID);

            responseText = Constants.INTENSE_TALK[0] + "oooh" + Constants.INTENSE_TALK[1];

            responseText += " Veo que son " + numberOfParticipants + " personas. " + "Esto estara interesante. Comencemos ";

            in = FunctionApi.getInstance()
                    .sendGet(FunctionApi.getInstance().UNIVERSAL_URL + "/firstQuestion", params);

            if(in==null) throw new NullPointerException();
            response = new JsonParser().parse(in).getAsJsonObject();

            String QuestionID = response.get(Parameters.QUESTION_ID).getAsString();

            sessionAttributes.put(Attributes.QUESTION_ID,QuestionID);

            String currentPlayer = response.get(Parameters.PLAYER).getAsString();

            String question = response.get(Parameters.QUESTION).getAsString();

            sessionAttributes.put(Attributes.CURRENT_PLAYER,currentPlayer);

            responseText += " . ";

            responseText += " El primero en responder sera " + currentPlayer + ".Tienes 10 segundos para prepararte. <break time='10s'/>";

            responseText += ". En sus marcas, listos, fuera. Tu pregunta es: " + question;

            return input.getResponseBuilder()
                    .withShouldEndSession(false)
                    .withSpeech(responseText)
                    .withReprompt(Constants.HELP_MESSAGE)
                    .build();

        } catch (Exception e) {
            e.printStackTrace();
        }

        return input.getResponseBuilder()
                .withShouldEndSession(true)
                .withSpeech("Hubo un error al iniciar sesion")
                .build();

    }
}
