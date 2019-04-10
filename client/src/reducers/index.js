import { combineReducers } from 'redux';

// my reducers
import authReducer from './authReducer';
import errorReducer from './errorReducer';

export default combineReducers ({

    auth: authReducer,
    errors: errorReducer,

});