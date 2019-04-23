const express = require("express");
const router = express.Router ();
const ejs = require('ejs');
const request = require("superagent");


router.get('/test', (req,res)=>{
    var accessToken = req.query.token;
    res.render('index',{accessToken:accessToken});
    //res.sendfile('./routes/api/index.html');
})

router.get('/oauth',(req,res)=>{
    var accessTokenAlexa = req.query.token;
    var accessTokenAmazon = req.query.access_token;
    
    res.render('./routes/api/login.html');
})

router.get('/start',(req,res)=>{
    console.log("In start");
        var info = {
            'participants':'20',
            'sessionID':'ABCDEFG',
            'lobbyID':'HIJ'
        }
        res.send(info);
})

router.get('/assets/startAudio',(req,res)=>{
    
})


router.get('/firstQuestion',(req,res)=>{
    console.log("In first");
    var info = {
        'questionID':'1',
        'question':'Quien es el presidente de algoritmia',
        'player':"Juan"
    }

    res.send(info);
});

router.post('/answer',(req,res)=>{
    
    console.log(
        "QuestionID: " + req.body.questionID + "\n" + 
        "Answer: " + req.body.answer + "\n"
        + "Current Player: " + req.body.player + "\n" +
        "Token: " + req.body.token
    );
    

    res.send({"status":"correct","nextPlayer":"pedro"});
})

router.get('/getNextQuestion',(req,res)=>{

    console.log("in next question + \n" + 
            "sessionID: " + req.query.sessionID);

    var info = {"questionID":"2","question":"Como me llamo"};

    res.send(info);
})

router.get('/end',(req,res)=>{
    console.log("in end");
    var info = {"winner":"juan", "points":"10"};

    res.send(info);
})


module.exports = router;