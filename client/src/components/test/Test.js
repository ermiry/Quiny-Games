import React, { Component } from 'react';

class Test extends Component {

    constructor () { super (); }

    // componentDidMount() {
    //     loadJS(
    //         'https://assets.loginwithamazon.com/sdk/na/login1.js'
    //     ) 
    //   }
    
    // loadJS (src) {
    //     amazon.Login.setClientId ('amzn1.application-oa2-client.27ec2ca814dc4d0585a9da7596b1fffb');
    //   const a = window.document.createElement('script'); a.type = 'text/javascript';
    //   a.async = true; a.id = 'amazon-login-sdk';
    //     a.src = 'https://assets.loginwithamazon.com/sdk/na/login1.js';
    //     window.document.getElementById('amazon-root').appendChild(a);
    //   const script = window.document.createElement ("script") 
    //   script.src = src 
    //   script.async = true 
    //   ref.parentNode.insertBefore(script, ref) 
    // }

    // test () {
    //     options = { scope : 'profile' };
    //     amazon.Login.authorize(options,function(response){
    //         if(response.error){
    //             alert('oauth error ' + response.error);
    //             return;
    //         }
    //         alert('Access Token:' + accessToken);
    //         console.log('Access Token: ' + response.access_token);



    //         //get quiny.games/api/game/test2?accesToken&response.access_token
    //         amazon.Login.retrieveProfile(accessToken,function(response){
    //             alert('Hello ' + response.profile.Name);
    //             alert('Your email is ' + response.profile.PrimaryEmail);
    //             alert('Your unique ID is' + response.profile.CustomerID);
    //             if(window.console && window.console.log)
    //                 window.console.log(response);
    //         })
    //     });

    //     return false;

    // }
  
    render () {
        return (
            <div>
                <h1>Test</h1>

                <div id="amazon-root"></div>

                <a onClick={this.test} id="LoginWithAmazon">
                    <img alt="Login with Amazon"
                        src="https://images-na.ssl-images-amazon.com/images/G/01/lwa/btnLWA_gold_156x32.png"
                        width="156" height="32" />
                </a>

            </div>
        );
    }
}

export default Test;