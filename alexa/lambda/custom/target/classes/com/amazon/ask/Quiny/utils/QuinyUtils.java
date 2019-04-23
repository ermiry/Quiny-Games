package com.amazon.ask.Quiny.utils;

import com.amazon.ask.dispatcher.request.handler.HandlerInput;
import com.amazon.ask.model.Response;
import com.amazon.ask.Quiny.model.Attributes;
import com.amazon.ask.Quiny.model.Constants;

import java.util.Map;
import java.util.Optional;
import java.util.Random;

public class QuinyUtils {

    public static String getAccessToken(HandlerInput input) {
        String accessToken;

        try {
            Map<String, Object> sessionAttributes = input.getAttributesManager().getSessionAttributes();
            accessToken = sessionAttributes.get(Attributes.ACCESS_TOKEN).toString();
        } catch (NullPointerException e) {
            accessToken = "null";
        }
        return accessToken;
    }

    public static Optional<Response> listenOptions(HandlerInput input) {
        String responseText;

        responseText = "Puedes escoger entre los siguientes juegos: Conocetón. Matetón y Quien Sabe Más, Escoge uno";

        return input.getResponseBuilder()
                .withSpeech(responseText)
                .withReprompt(Constants.HELP_MESSAGE)
                .withShouldEndSession(false)
                .withSimpleCard("Juegos", "Conoceton\nMateton\nQuien Sabe Mas")
                .build();

    }

    public static String getResponse(boolean correct) {

        if (correct)
            return Constants.CORRECT_RESPONSES.get(new Random().nextInt(Constants.CORRECT_RESPONSES.size()));
        else
            return Constants.INCORRECT_RESPONSES.get(new Random().nextInt(Constants.INCORRECT_RESPONSES.size()));
    }

    public static Optional<Response> noToken(HandlerInput input) {
        return input.getResponseBuilder()
                .withSpeech("No estas registrado, por favor registrate en la app de alexa")
                .withLinkAccountCard()
                .withShouldEndSession(true)
                .build();
    }
}
