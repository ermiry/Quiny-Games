package com.amazon.ask.Quiny.Api;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.Iterator;
import java.util.Map;

public class FunctionApi {
    private HttpURLConnection connection = null;
    private final String USER_AGENT = "Mozilla/5.0";
    public final String UNIVERSAL_URL = "https://fcaeee10.ngrok.io/api/quiny";
    private static FunctionApi instance;

    public static FunctionApi getInstance() {
        if (instance == null) instance = new FunctionApi();
        return instance;
    }

    public BufferedReader sendGet(String url, Map<String, String> params) throws IOException {
        if (params != null && params.size() > 0) {
            url += "?";
            Iterator<String> paramsItertor = params.keySet().iterator();
            while (paramsItertor.hasNext()) {
                String key = paramsItertor.next();
                String value = params.get(key);
                url += key + "=" + value;
                if (paramsItertor.hasNext()) url += "&";
                System.out.println(url);
            }
        }
        URL obj = new URL(url);
        connection = (HttpURLConnection) obj.openConnection();
        connection.setRequestMethod("GET");
        connection.setRequestProperty("User-Agent", USER_AGENT);
        connection.setRequestProperty("Content-Type", "text/plain");
        connection.setRequestProperty("charset", "utf-8");


        int responseCode = connection.getResponseCode();

        System.out.println("\nSending 'Get' request to URL: " + url);
        System.out.println("Response Code:" + responseCode);
        BufferedReader in;
        if (responseCode != 404) {
            in = new BufferedReader(
                    new InputStreamReader(connection.getInputStream())
            );
        } else {
            in = null;
        }
        return in;

    }

    public BufferedReader sendDelete(String url, Map<String, String> params) throws IOException {
        if (params != null && params.size() > 0) {
            url += "?";
            Iterator<String> paramsItertor = params.keySet().iterator();
            while (paramsItertor.hasNext()) {
                String key = paramsItertor.next();
                String value = params.get(key);
                url += key + "=" + value;
                if (paramsItertor.hasNext()) url += "&";
                System.out.println(url);
            }
        }
        URL obj = new URL(url);
        connection = (HttpURLConnection) obj.openConnection();
        connection.setUseCaches(false);
        connection.setDoInput(true);
        connection.setRequestMethod("DELETE");
        connection.setRequestProperty("User-Agent", USER_AGENT);
        connection.setRequestProperty("Content-Type", "text/plain");
        connection.setRequestProperty("charset", "utf-8");
        int responseCode = connection.getResponseCode();
        System.out.println("Sending 'Delete' to url: " + url);
        System.out.println("Response Code:" + responseCode);
        BufferedReader in = new BufferedReader(
                new InputStreamReader(connection.getInputStream()));
        return in;
    }

    public BufferedReader sendPost(String url, Map<String, String> params) throws IOException {
        URL obj = new URL(url);
        connection = (HttpURLConnection) obj.openConnection();
        connection.setUseCaches(false);
        connection.setDoInput(true); //true indicates the server returns response
        connection.setRequestMethod("POST");
        StringBuffer requestParams = new StringBuffer();
        System.out.println("Sending 'POST' to " + url);
        if (params != null && params.size() > 0) {
            connection.setDoOutput(true); // true indicates POST request
            Iterator<String> paramIterator = params.keySet().iterator();
            while (paramIterator.hasNext()) {
                String key = paramIterator.next();
                String value = params.get(key);
                requestParams.append(URLEncoder.encode(key, "UTF-8"));
                requestParams.append("=");
                requestParams.append(URLEncoder.encode(value, "UTF-8"));
                if (paramIterator.hasNext()) requestParams.append("&");
                System.out.println(requestParams);
            }
        }
        //SEND Post Data
        OutputStreamWriter writer = new OutputStreamWriter(
                connection.getOutputStream());
        writer.write(requestParams.toString());
        writer.flush();
        int responseCode = connection.getResponseCode();
        System.out.println(responseCode);
        BufferedReader in;
        if (responseCode != 404) {
            in = new BufferedReader(
                    new InputStreamReader(connection.getInputStream()));
        } else in = null;
        return in;
    }

    public void disconnect() {
        if (connection != null) connection.disconnect();
    }


}
