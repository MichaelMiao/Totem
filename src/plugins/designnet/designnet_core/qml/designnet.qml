import QtQuick 2.3
import QtQuick.Controls 1.2

ScrollView {

    anchors.fill: parent
    Canvas {
        id : canvas
        x : 10
        y : 10
        width: 100
        height: 100

        onPaint: {
            var ctx = getContext("2d");
            ctx.fillStyle = "red";
            ctx.shadowColor = "yellow";
            ctx.shadowBlur = 5;
            ctx.beginPath();
            var radius = canvas.width / 2;
            ctx.arc(radius, radius, radius, 0, Math.PI * 2, true);
            ctx.closePath();
            ctx.fill();
        }
    }
}
