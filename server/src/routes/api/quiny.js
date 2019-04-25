const express = require("express");
const router = express.Router ();
const ejs = require('ejs');

const auth0 = require ('auth0-js');
const reqq = require ('axios');

var token;
var userProfile;


router.get('/test', (req,res)=>{
    var accessToken = req.query.token;
    res.render('index',{"accessToken":accessToken});
    //res.sendfile('./routes/api/index.html');
    
})

router.get ('/oauth',(req,res)=>{
  console.log("request to oauth");
  //var accessTokenAlexa = req.query.token;
  //var accessTokenAmazon = req.query.access_token;
  var code = req.query.code;
  request.post('https://dev-qpzoqcez.auth0.com/oauth/token')
  .send(
    {
      "grant_type":"authorization_code",
      "code":code,
      "client_id":"ek5yWI1JyCXb78obHKzyfpdR14pISo0S",
      "client_secret":"3xUciwo_7iMhyqGwa3BTi1I71sZTzyxsKgo1mkV7UDKc3Afis17c6qN_qibEQjum",
      "redirect_uri": "https://9478a70d.ngrok.io/api/quiny/welcome"
    })
  .then(result => {
    token = result.body.access_token;
      console.log(result.body.access_token  +"\n tokenID: " + result.body.id_token)
      
      //return result;
  })
  .catch(error=>{
    console.error(error);
    console.log("error")
  })
  //var info = amazon.getProfile(accessTokenAmazon);
  
  //console.log(info)
  res.render('login',{"token":req.query.access_token});
});

// FIXME: call this after we recieve the token
router.get('/profile',(req,res)=>{
  var config = {
    headers:{'Authorization': 'Bearer ' + token}
  }
  console.log(token);
  
  reqq.default.get("https://dev-qpzoqcez.auth0.com/userinfo",config)
  .then(result=>{
    console.log(result);
    // TODO: get the user email and use it to add the user to add the token to the 
    // player info in the cerver

    // redirect back to react
  })
  .catch(error=>{
    console.error("error" + error.status);
  })
});


router.get("/welcome",(req,res)=>{
  console.log("welcome");
  res.send("welcome");
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



router.get('/firstQuestion',(req,res)=>{
    console.log("In first");
    var info = {
        'questionID':'1',
        'question':'Grupo del plato del buen comer el cual proporcionan energía',
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

    var info = {"questionID":"2","question":"Es la cantidad de calor de un cuerpo"};

    res.send(info);
})

router.get('/views/apl',(req,res)=>{
  console.log("Pruea");
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

module.exports = router;