package com.amazon.ask.Quiny.model;

import java.util.Arrays;
import java.util.List;

public class Constants {
    public static final String INTENSE_TALK[] = {"<say-as interpret-as='interjection'>", "</say-as><break strength='strong'/>"};
    public static final String WELCOME_MESSAGE = "Bienvenidos, Bienvenidas todos a Cuiny, donde aprender es más divertido";
    public static final String INFO_MESSAGE = "Si quieres escuchar los juegos que tengo para tí, dí, Escuchar juegos disponibles, si ya sabes que quieres jugar, a continuación di el nombre del juego";
    public static final String HELP_MESSAGE = "Si necesitas ayuda en algo, dí, Ayuda, y con gusto te ayudaré";
    public static final List<String> CORRECT_RESPONSES = Arrays.asList("hurra", "loteria", "mamma mia", "oh lala", "oi nomas", "órale", "perfectísimo", "perfecto", "ps si", "que alegría", "bravo");
    public static final List<String> INCORRECT_RESPONSES = Arrays.asList("hmm", "lastima", "lo lamento", "me da mucha pena", "mmm nah", "neeel", "nel", "ojala", "ora tú", "oye que te pasa", "por porquito");
    public static final String LAST_QUESTION = "la ultima y nos vamos";
    public static final String ACCESS_TOKEN = "token";
    public static int CONOCETON = 1;
    public static int MATETON = 2;
    public static int QUIEN_SABE_MAS = 3;
    public static String AUDIO[] = {"<audio src=", "/>"};


}

