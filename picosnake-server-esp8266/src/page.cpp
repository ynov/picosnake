#include "page.h"

#include <Arduino.h>

const char* IRAM_ATTR PageContent = R"(<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>PicoSnake</title>
    <style>
      * {
        font-family: monospace;
        box-sizing: border-box;
      }

      body {
        display: flex;
        justify-content: center;
        align-items: center;
        height: 100vh;
        padding: 0;
        margin: 0;
        background-color: #a9a9a9;
      }

      .container {
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        text-align: center;
        width: 320px;
        height: 320px;
        background-color: #f0f0f0;
        position: relative;
        border-radius: 5%;
        box-shadow: 4px 4px 10px 2px #626262;
      }

      .joystick {
        width: 60%;
        height: 60%;
        background-color: #666;
        background: linear-gradient(to bottom, #666, #333);
        box-shadow: inset 0 0 10px rgba(0, 0, 0, 0.5);
        border-radius: 50%;
        position: absolute;
        touch-action: none;
        user-select: none;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
      }

      .joystick:hover {
        cursor: pointer;
      }

      .reset-button {
        position: absolute;
        top: 10px;
        right: 10px;
        background-color: #f0f0f0;
        padding: 5px 10px;
        border-radius: 5px;
        border: 1px solid #666;
      }
    </style>
  </head>

  <body>
    <button onclick="window.cmd.reset() " class="reset-button">Reset</button>
    <div class="container">
      <div class="joystick"></div>
    </div>

    <script>
      let currentDirection = "none";

      window.cmd = {
        move: function (direction) {
          console.log(`move: ${direction}`);
          window.fetch(`/${direction}`, { method: "GET" });
        },
        reset: function () {
          console.log("reset");
          window.fetch(`/reset`, { method: "GET" });
        },
      };

      const joystick = document.querySelector(".joystick");
      const container = document.querySelector(".container");
      let isDragging = false;
      let currentX = 0;
      let currentY = 0;
      let initialX;
      let initialY;
      let xOffset = 0;
      let yOffset = 0;

      function dragStart(e) {
        if (e.type === "touchstart") {
          initialX = e.touches[0].clientX - xOffset;
          initialY = e.touches[0].clientY - yOffset;
        } else {
          initialX = e.clientX - xOffset;
          initialY = e.clientY - yOffset;
        }

        if (e.target === joystick) {
          isDragging = true;
        }
      }

      function dragEnd() {
        xOffset = 0;
        yOffset = 0;
        setTranslate(0, 0, joystick);
        isDragging = false;
        setTimeout(() => {
          currentDirection = "none";
        }, 10);
      }

      let timeout;

      function drag(e) {
        if (isDragging) {
          e.preventDefault();

          if (e.type === "touchmove") {
            currentX = e.touches[0].clientX - initialX;
            currentY = e.touches[0].clientY - initialY;
          } else {
            currentX = e.clientX - initialX;
            currentY = e.clientY - initialY;
          }

          const containerRect = container.getBoundingClientRect();
          const maxRadius = containerRect.width * 0.3;

          const distance = Math.sqrt(currentX * currentX + currentY * currentY);

          if (distance > maxRadius) {
            const scale = maxRadius / distance;
            currentX *= scale;
            currentY *= scale;
          }

          xOffset = currentX;
          yOffset = currentY;

          let angle = Math.atan2(currentY, currentX) * (180 / Math.PI);

          if (angle < 0) {
            angle += 360;
          }

          let nextDirection = "none";

          if (angle > 45 && angle < 135) {
            nextDirection = "down";
          } else if (angle > 135 && angle < 225) {
            nextDirection = "left";
          } else if (angle > 225 && angle < 315) {
            nextDirection = "up";
          } else if (angle > 315 || angle < 45) {
            nextDirection = "right";
          }

          if (nextDirection !== currentDirection) {
            currentDirection = nextDirection;
            window.cmd.move(currentDirection);
          }

          setTranslate(xOffset, yOffset, joystick);
        }
      }

      function setTranslate(xPos, yPos, el) {
        el.style.transform = `translate(calc(-50% + ${xPos}px), calc(-50% + ${yPos}px))`;
      }

      container.addEventListener("touchstart", dragStart, false);
      container.addEventListener("touchend", dragEnd, false);
      container.addEventListener("touchmove", drag, false);
      container.addEventListener("mousedown", dragStart, false);
      container.addEventListener("mouseup", dragEnd, false);
      container.addEventListener("mousemove", drag, false);
      container.addEventListener("mouseleave", dragEnd, false);
    </script>
  </body>
</html>
)";
