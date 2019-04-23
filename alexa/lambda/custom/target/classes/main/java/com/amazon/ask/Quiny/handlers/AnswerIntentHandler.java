package com.amazon.ask.Quiny.handlers;

import com.amazon.ask.Quiny.Api.FunctionApi;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;
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
        boolean correct = false, error = false;
        String currentPlayer;
        String responseText;
        String answer;
        String nextParticipant;
        String accessToken = QuinyUtils.getAccessToken(input);
        int counter = 0;


        if (accessToken != null) params.put(Constants.ACCESS_TOKEN, accessToken);
        else return QuinyUtils.noToken(input);

        BufferedReader in;
        try {
            counter = Integer.valueOf(sessionAttributes.get(Attributes.QUESTION_ID).toString());

            params.put("questionID", String.valueOf(counter));

            currentPlayer = (sessionAttributes.get(Attributes.CURRENT_PLAYER).toString());

            answer = slots.get("Answer").getValue();

            if (answer == null || answer.equals("")) throw new IOException();

            params.put(Constants.ACCESS_TOKEN, accessToken);

            params.put("answer", answer);

            params.put("player", currentPlayer);

            in = FunctionApi.getInstance()
                    .sendPost(FunctionApi.getInstance().UNIVERSAL_URL + "/api/games/question", params);

            JsonObject response = new JsonParser().parse(in).getAsJsonObject();

            if (!response.has("status")) throw new NullPointerException();

            String status = response.get("status").getAsString();

            nextParticipant = response.get("next").getAsString();

            if (nextParticipant == null) throw new NullPointerException();

            if (nextParticipant.equals("end")) nextParticipant = "end";

            if (status.equals("correct")) correct = true;

            responseText = QuinyUtils.getResponse(correct);
            if (correct)
                responseText += ". La respuesta es correcta. Haz ganado un punto.";
            else
                responseText += ". La respuesta fue incorrecta. No haz ganado nada.";


            if (!nextParticipant.equals("end")) {

                responseText += "El siguiente en jugar es: " + nextParticipant + ". Tienes 10 segundos para prepararte. <break time='10s'/>";

                sessionAttributes.put(Attributes.CURRENT_PLAYER, nextParticipant);

                String question;

                params.remove("answer");

                params.remove("player");

                in = FunctionApi.getInstance()
                        .sendGet(FunctionApi.getInstance().UNIVERSAL_URL + "/getNextQuestion", params);

                response = new JsonParser().parse(in).getAsJsonObject();

                if (!response.has("question")) throw new IOException();

                if (!response.has("questionId")) throw new IOException();

                question = response.get("question").getAsString();

                responseText += ". En sus marcas, listos, fuera. Tu pregunta es: " + question;

                sessionAttributes.put(Attributes.QUESTION_ID, response.get("questionId").getAsString());
            } else {
                responseText += "Listo, han terminado";
                sessionAttributes.put(Attributes.STATE, Attributes.END_STATE);

            }

            return input.getResponseBuilder()
                    .withSpeech(responseText)
                    .withShouldEndSession(false)
                    .build();

        } catch (Exception e) {

            e.printStackTrace();

            System.out.println("An error was ocurred while posting the answer");

            return input.getResponseBuilder()
                    .withSpeech("No se ha podido procesar la información correctamente, lamentablemente no podremos continuar, una disculpa, Adios")
                    .withShouldEndSession(true)
                    .build();
        }
    }
}
