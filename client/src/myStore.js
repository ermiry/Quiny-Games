import { createStore, applyMiddleware, compose } from 'redux';

import thunk from 'redux-thunk';

// my reducers
import rootReducer from './reducers';

let initialState = {};

const middleware = [thunk];

// FIXME: remove devtools when in production!!!
const myStore = createStore (
    rootReducer, 
    initialState, 
    compose (applyMiddleware (...middleware),
    window.__REDUX_DEVTOOLS_EXTENSION__ && window.__REDUX_DEVTOOLS_EXTENSION__ ()));

export default myStore;