const mongoose = require ('mongoose');
const Schema = mongoose.Schema;

const GroupSchema = new Schema ({

    name: {
        type: String,
        required: true
    },
    leader: {
        type: Schema.Types.ObjectId,
        ref: 'users'
    },
    members: [
        {
            type: Schema.Types.ObjectId,
            ref: 'users'
        }
    ]
});

module.exports = Group = mongoose.model ('groups', GroupSchema);