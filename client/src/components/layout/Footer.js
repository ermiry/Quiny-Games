import React from 'react';

export default () => {

    var style = {
        padding: "20px",
        position: "fixed",
        left: "0",
        bottom: "0",
        height: "60px",
        width: "100%",
        background: "#1e824c",
    }
    
    var phantom = {
        display: 'block',
        padding: '20px',
        height: '60px',
        width: '100%',
    }

    return (
        <div>
            <div style={phantom} />
                <footer style={style} className="text-white mt-5 p-4 text-center">
                    Copyright &copy; Quiny { new Date().getFullYear() }
                </footer>
        </div>
    );

};