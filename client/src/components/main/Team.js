import React, { Component } from 'react';

class Team extends Component {
  
    render () {
        return (
            <div className="team">
                <div className="container">
                <h1 className="heading">Our Team</h1>
                    <br/>
                    <div className="card-wrapper">
                        <div className="card"> 
                            <img src="img//ArturoCasillas.jpeg" alt="ArturoCasillas" className="profile-img"/>
                            <h1>Casillas Arutro</h1>
                            <p className="job-title">APL Developer</p>
                            <p className="about">Programmer of Alexa Multimodels Skill and Interactions </p>
                        </div>
                        <div className="card"> 
                            <img src="img//LuisHuerta.jpeg" alt="LuisHuerta" className="profile-img"/>
                            <h1>Huerta Luis</h1>
                            <p className="job-title">Data Base Manager</p>
                            <p className="about">Designer and manager of the data base of Quiny Games</p>
                        </div>
                        <div className="card"> 
                            <img src="img//JuanLara.jpeg" alt="JuanLara" className="profile-img"/>
                            <h1>Lara Juan Carlos</h1>
                            <p className="job-title">Alexa Skill Expert</p>
                            <p className="about">Programmer of the skill of Quiny Games</p>
                        </div>
                        <div className="card"> 
                            <img src="img//KarenPonce.jpeg" alt="KarenPonce" className="profile-img"/>
                            <h1>Ponce Karen</h1>
                            <p className="job-title">Frontend Developer</p>
                            <p className="about">Programmer and designer of the frontend of Quiny Games</p>
                        </div>
                        <div className="card"> 
                            <img src="img//ErickSalas.jpeg" alt="Erick" className="profile-img"/>
                            <h1>Salas Erick</h1>
                            <p className="job-title">Leading Developer</p>
                            <p className="about">Wrote a custome server framework in C and developer of the Quiny Games API that connects the frontend with the backend</p>
                        </div>
                    </div>
                    <br/>
                </div>
            </div>
        );
    }
}

export default Team;