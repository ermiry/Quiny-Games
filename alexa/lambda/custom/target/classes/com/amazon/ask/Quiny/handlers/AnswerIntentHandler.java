package com.amazon.ask.Quiny.handlers;

import com.amazon.ask.Quiny.Api.FunctionApi;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;
import com.amazon.ask.Quiny.model.Parameters;
import com.amazon.ask.Quiny.utils.QuinyUtils;
import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.model.IntentRequest;
import com.amazon.ask.model.Response;
import com.amazon.ask.model.Slot;
import com.amazon.ask.request.Predicates;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;


//TODO:
public class AnswerIntentHandler implements com.amazon.ask.dispatcher.request.handler.RequestHandler {
    @Override
    public boolean canHandle(HandlerInput input) {
        return input.matches(Predicates.intentName("AnswerIntent")
                .and(Predicates.sessionAttribute(Attributes.STATE, Attributes.QUIZ_STATE)));
    }

    @Override
    public Optional<Response> handle(HandlerInput input) {
        IntentRequest intentRequest = (IntentRequest) input.getRequestEnvelope().getRequest();
        Map<String, Object> sessionAttributes = input.getAttributesManager().getSessionAttributes();
        Map<String, String> params = new HashMap<>();
        Map<String, Slot> slots = intentRequest.getIntent().getSlots();
        boolean correct = false;
        String currentPlayer;
        String responseText;
        String answer;
        String nextParticipant;
        String accessToken = QuinyUtils.getAccessToken(input);
        String sessionID = sessionAttributes.get(Attributes.SESSION_ID).toString() ;

        //TODO: To connect players and session, they introduce their account to Quiny.games, and they put the lobbyID
        //This will return them a sessionID, that will be saved in the user and the attributes

        //TODO: Stop sending accessToken and send sessionToken
        if (accessToken != null) params.put(Constants.ACCESS_TOKEN, accessToken);
        else return QuinyUtils.noToken(input);

        BufferedReader in;
        try {
            String questionID = (sessionAttributes.get(Attributes.QUESTION_ID).toString());

            params.put("questionID", questionID);

            currentPlayer = (sessionAttributes.get(Attributes.CURRENT_PLAYER).toString());

            answer = slots.get("Answer").getValue();

            if (answer == null || answer.equals("")) throw new IOException();

            params.put(Constants.ACCESS_TOKEN, accessToken);

            params.put(Parameters.ANSWER, answer);

            params.put(Parameters.PLAYER, currentPlayer);

            in = FunctionApi.getInstance()
                    .sendPost(FunctionApi.getInstance().UNIVERSAL_URL + "/answer", params);

            JsonObject response = new JsonParser().parse(in).getAsJsonObject();

            if (!response.has(Parameters.STATUS)) throw new NullPointerException();

            String status = response.get(Parameters.STATUS).getAsString();

            nextParticipant = response.get(Parameters.NEXT_PARTICIPANT).getAsString();

            if (nextParticipant == null) throw new NullPointerException();

            if (nextParticipant.equals(Parameters.RESPONSE_END)) nextParticipant = "end";

            if (status.equals(Parameters.RESPONSE_CORRECT)) correct = true;

            responseText = QuinyUtils.getResponse(correct);
            if (correct)
                responseText += " . La respuesta es correcta. Haz ganado un punto. ";
            else
                responseText += " . La respuesta fue incorrecta. No haz ganado nada. ";


            if (!nextParticipant.equals(Parameters.RESPONSE_END)) {

                responseText += "El siguiente en jugar es: " + nextParticipant + ". Tienes 10 segundos para prepararte. <break time='10s'/>";

                sessionAttributes.put(Attributes.CURRENT_PLAYER, nextParticipant);

                String question;

                params.remove(Parameters.PLAYER);
                params.remove(Parameters.ANSWER);
                params.remove(Parameters.QUESTION_ID);


                in = FunctionApi.getInstance()
                        .sendGet(FunctionApi.getInstance().UNIVERSAL_URL + "/getNextQuestion", params);

                response = new JsonParser().parse(in).getAsJsonObject();

                if (!response.has(Parameters.QUESTION)) throw new IOException();

                if (!response.has(Parameters.QUESTION_ID)) throw new IOException();

                question = response.get(Parameters.QUESTION).getAsString();

                responseText += ". En sus marcas, listos, fuera. Tu pregunta es: " + question;

                sessionAttributes.put(Attributes.QUESTION_ID, response.get(Parameters.QUESTION_ID).getAsString());


            } else {
                responseText += " Listo, han terminado";
                sessionAttributes.put(Attributes.STATE, Attributes.WAITING);
                sessionAttributes.put(Attributes.RETRY,true);
                in = FunctionApi.getInstance().sendGet(
                        FunctionApi.getInstance().UNIVERSAL_URL + "/end",params);
                response = new JsonParser().parse(in).getAsJsonObject();
                if(!response.has("winner")) throw new IOException();
                if(!response.has("points")) throw new IOException();
                String winner = response.get("winner").getAsString();
                String points = response.get("points").getAsString();
                responseText += ". El ganador fue " + winner + " Con " + points;
            }
            return input.getResponseBuilder()
                    .withSpeech(responseText)
                    .withShouldEndSession(false)
                    .build();

        } catch (Exception e) {

            e.printStackTrace();

            System.out.println("An error was ocurred while posting the answer");
            return input.getResponseBuilder()
                    .withSpeech("No se ha podido procesar la informaci��n correctamente, lamentablemente no podremos continuar, una disculpa, Adios")
                    .withShouldEndSession(true)
                    .build();
        }
    }
}
