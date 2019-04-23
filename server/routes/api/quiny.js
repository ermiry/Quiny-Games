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
        var info = {
            'participants':'20',
            'sessionID':'ABCDEFG',
            'lobbyID':'HIJ'
        }
        res.send(info);
})


router.get('/firstQuestion',(req,res)=>{
    var info = {
        'question_id':'1',
        'question':'Quien es el presidente de algoritmia',
        'player':"Juan"
    }

    res.send(info);
});

router.post('/question',(req,res)=>{
    console.log(
        "Answer: " + req.params.answer + "\n"
        + "Current Player: " + req.params.player
    );

    
    res.send({"status":"correct","next":"Pedro"});
})

router.get('getNextQuestion',(req,res)=>{
    res.send({"question_id":"2","question":"Como me llamo"
    })
})





router.get
module.exports = router;