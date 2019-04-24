const express = require("express");
const router = express.Router ();
const ejs = require('ejs');
const request = require("superagent");
const fs = require('fs');
const amazon = require('login-with-amazon');
const aws = require('aws-sdk');


router.get('/test', (req,res)=>{
    var accessToken = req.query.token;
    res.render('index',{"accessToken":accessToken});
    //res.sendfile('./routes/api/index.html');
    
})

router.get('/oauth',(req,res)=>{
  console.log("request to oauth");
  //var accessTokenAlexa = req.query.token;
  var accessTokenAmazon = req.query.access_token;
  
  
  var info = amazon.getProfile(accessTokenAmazon);
  
  console.log(info)
  res.render('login',{"token":req.query.access_token});
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

router.get('/views/apl',(req,res)=>{
    var apl = {
        "type": "APL",
        "version": "1.0",
        "theme": "dark",
        "import": [],
        "resources": [],
        "styles": {},
        "layouts": {},
        "mainTemplate": {
          "items": [
            {
              "type": "Frame",
              "backgroundColor": "grey",
              "item": {
                "type": "Container",
                "height": "100vh",
                "width": "100vw",
                "direction": "column",
                "alignItems": "center",
                "justifyContent": "center",
                "items": [
                  {
                    "type": "Text",
                    "text": "Nutrición Inteligente"
                  },
                  {
                    "type": "Text",
                    "text": "¡Bienvenidos!"
                  }
                ]
              }
            }
          ]
        }
      }
      res.send(apl);
})

router.get('/end',(req,res)=>{
    console.log("in end");
    var info = {"winner":"juan", "points":"10"};

    res.send(info);
})

router.get('/audio',(req,res)=>{
    console.log("audio");
    var fileId = req.query.fileId;
    var file = __dirname + '/audio/' + fileId;
    fs.exists(file,function(exists){
        if(exists)
        {
            res.setHeader('Content-disposition','attachment; filename = ' + fileId);
            res.setHeader('Content-Type','application/audio/mpeg3');            
            var rstream = fs.createReadStream(file);
            rstream.pipe(res);

        }else
        {
            res.send("Its a 404");
            res.end();
        }
    });
});

module.exports = router;