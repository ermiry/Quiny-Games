import React, { Component } from 'react';

class Games extends Component {
  
    render () {
        return (
            <div>
            <div className="home">
                <div className="landing-text">
                        <h1 className="heading">¿Quién Sabe Mas?</h1>
                        <h3 className="subheading">Be the Smartest</h3>
                        <div className="wrapper">
                                <div className="row">
                                    <div className="col"><a href="javascript:void(0)" onclick="toggle_visibility('create');" id="button" className="btn">Create Game</a></div>
                                    <div className="col"><a href="javascript:void(0)" onclick="toggle_visibility('join');" id="button" className="btn ">Join Game</a></div>
                                    <div className="col"><a href="javascript:void(0)" onclick="toggle_visibility('leaderboards');" id="button" className="btn">See LeaderBoards</a> </div>
                                </div>
                        </div>
                </div>
            </div>
            <div className="padding">
                <div className="container">
                    <div className="row">
                        <div className="col">
                            <p className="inf-img">Smart</p>
                            <img src="img//brain.jpeg" alt="Place Holder" className="minimalist-img"/>
                            
                        </div>
                        <div className="col">
                            <p className="inf-img">Group</p>
                            <img src="img//people.jpeg" alt="Place Holder" className="minimalist-img"/>
                            
                        </div>
                        <div className="col"> 
                        <p className="inf-img">Voice</p>
                            <img src="img//talking.jpeg" alt="Place Holder" className="minimalist-img"/>  
                        </div>
                    </div>
                </div>
            </div>
            <div className="information">       
                <div className="container">
                    <div className="row">
                        <div className="col">
                                <h2 className="rules">Las Reglas</h2> 
                                <p className="rules-info">Las reglas del juego son sencillas,
                                        primero el profesor debe crear una
                                        nueva sesión del juego, en donde
                                        podrá elegir varias configuraciones
                                        del juego como el tema, ordenados
                                        por materias, también podrá
                                        configurar el número de jugadores y la
                                        manera en la que se interactuará con
                                        el juego. Sus alumnos podrán tener
                                        acceso de manera fácil utilizando un
                                        link en la interfaz gráfica y cuando la
                                        maestra considere pertinente, podrá
                                        inicializar la ejecución de la
                                        aplicación. Esto lo podrá lograr de dos
                                        maneras diferentes: por un comando
                                        de voz hacia Alexa para inicializar la
                                        aplicación haciendo uso de Amazon
                                        Alexa o de la interfaz gráfica.
                                        Ya en la ejecución de la aplicación,
                                        si se eligió la opción de interactuar
                                        con Amazon Alexa, ella podrá recibir
                                        las preguntas y el nombre del
                                        participante al que va dirigida cada
                                        una, en cambio si se hace uso de la
                                        interfaz gráfica, cada usuario estará
                                        jugando en su propia sesión y le
                                        llegaran las mismas preguntas a cada
                                        participante y cada uno podrá
                                        contestar.
                                        Sin importar la manera de
                                        interactuar con la aplicación, al final
                                        del juego, el usuario con mayor
                                        puntaje será el ganador y se
                                        desplegará una tabla con los
                                        participantes y sus puntajes.</p>
                        </div>
                        <div className="col">
                            <img src="img//MorePeople.PNG" alt="MorePeople" className="people"/>
                        </div>
                    </div>
                </div>
            </div>

            {/* <!-- Join Game --> */}
            <div id="join" className="bg-modal">
                    <div className="modal-content">
                        <div className ="close" href="javascript:void(0)"><a onclick="toggle_visibility('join');" /* style={{color: black, position: fixed, top: 0}} */ >+</a></div>
                    
                        <h1 className="mb-5">Join game</h1>
                        <h1 className="mb-5">Enter code:</h1>
                        <input type="text" name="code" className="field" required pattern="[a-zA-Z0-9]+" style={{width: 200 + 'px'}} />
                        <div className="mt-5 container "> <button className=" btn-go">
                                <h3>Ir al juego</h3>
                            </button></div>
            
                        
                    </div>
                </div>

                {/* <!-- Leaderboards --> */}
                <div id="leaderboards" className="bg-modal">
                    <div className="modal-content">
                            <div className="close"  href="javascript:void(0)"><a onclick="toggle_visibility('leaderboards');" style="color: black">+</a></div>

                            <div className="container ">
                                    <h1 className="display-2 text-center ">
                                        <img src="img/star.png" className="mb-4"/>
                                        Leaderboards
                                        <img src="img/star.png" className="mb-4"/></h1>
                                </div>
                            <div className="mt-5 container" style={{width: 800 + 'px'}}>
                                    <table className=" table tablescore table-hover table-dark mb-5">
                                        <thead>
                                            <tr /* style={{backgroundColor: slategray}} */>
                                                <th scope="col"><h3>#</h3></th>
                                                <th scope="col"><h3>Nombre</h3></th>
                                                <th scope="col"><h3>Puntuaje</h3></th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr className="ganador">
                                                <th scope="row"><h4>1</h4></th>
                                                <td><h4>Mark</h4></td>
                                                <td><h4>Otto</h4></td>
                                            </tr>
                                            <tr className="ganador">
                                                <th scope="row"><h4>1</h4></th>
                                                <td><h4>Mark</h4></td>
                                                <td><h4>Otto</h4></td>
                                            </tr>
                                            <tr className="ganador">
                                                <th scope="row"><h4>1</h4></th>
                                                <td><h4>Mark</h4></td>
                                                <td><h4>Otto</h4></td>
                                            </tr>
                                            <tr className="ganador">
                                                <th scope="row"><h4>1</h4></th>
                                                <td><h4>Mark</h4></td>
                                                <td><h4>Otto</h4></td>
                                            </tr>
                                            <tr className="ganador">
                                                <th scope="row"><h4>1</h4></th>
                                                <td><h4>Mark</h4></td>
                                                <td><h4>Otto</h4></td>
                                            </tr>
                                            
                                        </tbody>
                                    </table>
                                </div>
                            </div>
                    </div>

            {/* <!-- Create game --> */}
            <div id="create" className="bg-modal">
                <div className="modal-content">
                    <div className="close"  href="javascript:void(0)"><a onclick="toggle_visibility('create');" /* ={{color: black}} */>+</a></div>
                    <h1 className="mb-5">Create game</h1>
                    <div className="container">
                        <div className="mt-5 container">
                            <div className="row">
                                <div className="col-7" style="text-align: left">
                                    <h1>Seleccione un tema: </h1>
                                </div>
                                <div className="col-5">
                                    <div className=" box navigationDesktop" style={{width: 300 + 'px'}}>
                                        <select>
                                            <nav>
                                                <ul>
                                                <li> <a href="#">
                                                        <option value="1">Science</option>
                                                    </a></li>
                                                </ul>
                                                <option value="2">Maths</option>
                                                <option value="3">History</option>
                                            </nav>
                                        </select>
                                    </div>
                                </div>
                            <b className="m-4"></b>
                            </div>
                            <div className="row">
                                <div className="col-7" /* style={{textAlign: left}}*/>
                                    <h1>¿Cuantos participantes? </h1>
                                </div>
                                <div className="col-5" >
                                    <div className="box">
                                        <input type="number" value="5"/>
                                    </div>
                                </div>

                            </div>
                        </div>

                    <div className="m-5 container "> <button className=" btn-go">
                            <h3>Ir al juego</h3>
                        </button></div>
                </div>
            </div>
        </div>
        </div>
        );
    }
}

export default Games;