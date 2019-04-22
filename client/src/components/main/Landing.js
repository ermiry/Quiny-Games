import React, { Component } from 'react';

class Landing extends Component {
  
    render () {
        // style="background-color: rgba(255, 255, 255, 0.7)

        return (
            <div>
                <div className="jumbotron-fluid p-3 mb-2 text-white jumbotronc">
                    {/* <!-- <img src="img/f4.jpg" className="card-img" > card-img-overlay --> */}
                    <div className="container">
                        <h1 className="display-1">Quiny Games</h1>
                        <p className="lead">A fun and interactive way to boost children education.</p>
                    </div>

                </div>

                <div className="card border-0 shadow my-5">
                    <div className="card-body p-5" >
                        <div>
                            <div className="pt-5 container ">
                                <div className="row">
                                    <div className="col-sm">
                                        <img src="img/quiny-yellow.png" className="rounded-circle" alt="Quiny Logo" />

                                    </div>
                                    <div className=" col-sm mr-4">
                                        <h1 className="text-center"> ¿Que somos?</h1>
                                        <p className="text-center">Lorem, ipsum dolor sit amet consectetur adipisicing elit.
                                            Eveniet sequi error mollitia amet voluptas quasi in
                                            iusto ipsam! Voluptas sunt modi suscipit doloremque vero odit exercitationem
                                            necessitatibus.</p>
                                    </div>
                                    <div className="col-sm ml-3">
                                        <h5 className="display-4">Nuestros juegos</h5>
                                        <div className="pt-4 btn-group-vertical">
                                            <button type="button" className="btn myButtonazul btn-lg text-white">¿Quien sabe
                                                mas?</button>
                                            <button type="button" className="btn myButtonamarillo btn-lg text-white">Secondary</button>
                                            <button type="button" className="btn myButtonverdelimon btn-lg text-white">Success</button>

                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        );
    }
}

export default Landing;