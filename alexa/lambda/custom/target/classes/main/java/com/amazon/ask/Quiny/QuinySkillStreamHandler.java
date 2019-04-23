package com.amazon.ask.Quiny;

import com.amazon.ask.Quiny.handlers.*;
import com.amazon.ask.SkillStreamHandler;
import com.amazon.ask.Skills;

public class QuinySkillStreamHandler extends SkillStreamHandler {
    public QuinySkillStreamHandler() {
        super(Skills.standard()
                .addRequestHandlers(
                        new LaunchRequestHandler(),
                        new ExitIntentHandler(),
                        new HelpIntentHandler(),
                        new RepeatIntentHandler(),
                        new SessionEndedHandler(),
                        new AnswerIntentHandler(),
                        new OptionIntentHandler(),
                        new ListenOptionsIntentHandler(),
                        new YesNoIntentHandler(),
                        new QuienSabeMasStarterIntentHandler()
                ).build());
    }
}
