import React, { Component } from 'react';

class Landing extends Component {
  
    render () {
        // style="background-color: rgba(255, 255, 255, 0.7)

        return (
            <div>
                <div className="jumbotron-fluid p-3 mb-2 text-white jumbotronc">
                    {/* <!-- <img src="img/f4.jpg" class="card-img" > card-img-overlay --> */}
                    <div className="container">
                        <h1 className="display-1">Quiny Games</h1>
                        <p className="lead">This is a modified jumbotron that occupies the entire horizontal space of its parent.</p>
                    </div>

                </div>

                <div class="card border-0 shadow my-5">
                    <div class="card-body p-5" >
                        <div>
                                <div class="pt-5 container ">
                                    <div class="row">
                                        <div class="col-sm">
                                            <img src="img/quiny-yellow.png" class="rounded-circle" />

                                        </div>
                                        <div class=" col-sm mr-4">
                                            <h1 class="text-center"> ¿Que somos?</h1>
                                            <p class="text-center">Lorem, ipsum dolor sit amet consectetur adipisicing elit.
                                                Eveniet sequi error mollitia amet voluptas quasi in
                                                iusto ipsam! Voluptas sunt modi suscipit doloremque vero odit exercitationem
                                                necessitatibus.</p>
                                        </div>
                                        <div class="col-sm ml-3">
                                            <h5 class="display-4">Nuestros juegos</h5>
                                            <div class="pt-4 btn-group-vertical">
                                                <button type="button" class="btn myButtonazul btn-lg text-white">¿Quien sabe
                                                    mas?</button>
                                                <button type="button" class="btn myButtonamarillo btn-lg text-white">Secondary</button>
                                                <button type="button" class="btn myButtonverdelimon btn-lg text-white">Success</button>

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