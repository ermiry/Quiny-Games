const Validator = require ('validator');
const isEmpty = require ('./isEmpty');

// validate required user register fields
module.exports = function validateRegisterInput (data) {

    let errors = {};

    data.name = !isEmpty (data.name) ? data.name : '';
    data.email = !isEmpty (data.email) ? data.email : '';
    data.password = !isEmpty (data.password) ? data.password : '';
    data.password2 = !isEmpty (data.password2) ? data.password2 : '';

    // validate name
    if (Validator.isEmpty (data.name)) errors.name = 'Name field is required.';

    // validate email
    if (Validator.isEmpty (data.email)) errors.email = 'Email field is required.';
    else if (!Validator.isEmail (data.email))
        errors.email = 'Email is invalid.';

    // TODO: check for special characters
    // validate username
    if (Validator.isEmpty (data.username)) errors.username = 'Username field is required.';
    else if (!Validator.isLength (data.username, { min: 2, max: 30 }))
        errors.username = 'Username must be between 2 and 30 characters.';

    // validate password
    if (Validator.isEmpty(data.password)) errors.password = 'Password field is required.';
    else if (!Validator.isLength (data.password, { min: 6, max: 30 }))
        errors.password = 'Password must be at least 6 characters.';

    // validate password 2
    if (Validator.isEmpty (data.password2)) errors.password2 = 'Confirm Password field is required.';
    else if (!Validator.equals (data.password, data.password2))
        errors.password2 = 'Passwords must match.';

    // validate the role
    if (Validator.isEmpty (data.role)) errors.role = 'Role field is required.';
    else if (!Validator.equals (data.role, "student") || !Validator.equals (data.role, "teacher"))
        errors.role = 'Select a valid role.';

    return {
        errors,
        isValid: isEmpty (errors)
    };
};
