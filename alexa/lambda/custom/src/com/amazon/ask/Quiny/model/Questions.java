package com.amazon.ask.Quiny.model;

public class Questions {
    private String question;
    private String answer;
    private String hint;
    private String type;
    private boolean used;
    private int points;


    public Questions(String question, String answer, String hint, String type, int points) {
        this.question = question;
        this.answer = answer;
        this.hint = hint;
        this.type = type;
        this.used = false;
        this.points = points;
    }

    public String getQuestion() {
        return this.question;
    }

    public String getAnswer() {
        return this.answer;
    }

    public boolean isAnswerCorrect(String ans) {
        if (answer.compareTo(ans) == 0) return true;
        else return false;
    }

    public String getHint() {
        return this.hint;
    }

    public int getPoints() {
        return this.points;
    }

    public void useQuestion() {
        this.used = true;
    }
}
