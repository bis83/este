// Copyright (c) bis83. Distributed under the MIT License.

class Scene {
    constructor() {
        this.reset();
    }

    onload(input, fx) {
        fx.sprite.load("img/background.png");
        fx.sprite.load("img/board.png");
        fx.sprite.load("img/fountainhead.png");
        fx.sprite.load("img/fountains.png");
        fx.sprite.load("img/gameover.png");
        fx.sprite.load("img/prestart.png");
    }

    reset() {
        this.isGameStart = false;
        this.isGameOver = false;
        this.head = [];
    }

    update(input) {
    }

    draw(fx) {
        this.drawBackground(fx);
        this.drawBoard(fx);
        if(!this.isGameStart) {
            this.drawPreStart(fx);
        }
        if(this.isGameOver) {
            this.drawGameOver(fx);
        }
    }

    drawBackground(fx) {
        fx.sprite.texture("img/background.png");
        fx.sprite.uv(0, 0, 1, 1);
        fx.sprite.color(1, 1, 1, 1);
        fx.sprite.trans(400, 300, 800, 600, 0);
        fx.sprite.draw();
    }

    drawBoard(fx) {
        fx.sprite.texture("img/board.png");
        fx.sprite.uv(0, 0, 1, 1);
        fx.sprite.color(1, 1, 1, 1);
        fx.sprite.trans(300, 300, 500, 500, 0);
        fx.sprite.draw();
    }

    drawFountainHeads(fx) {
    }

    drawFountains(fx) {
    }

    drawFountain(fx, x, y, a) {
    }

    drawFountainCursor(fx) {
    }

    drawFountainNext(fx) {
    }

    drawScore(fx) {
    }

    drawPreStart(fx) {
        fx.sprite.texture("img/prestart.png");
        fx.sprite.uv(0, 0, 1, 1);
        fx.sprite.color(1, 1, 1, 1);
        fx.sprite.trans(400, 300, 800, 600, 0);
        fx.sprite.draw();
    }

    drawGameOver(fx) {
        fx.sprite.texture("img/gameover.png");
        fx.sprite.uv(0, 0, 1, 1);
        fx.sprite.color(1, 1, 1, 1);
        fx.sprite.trans(400, 300, 800, 600, 0);
        fx.sprite.draw();
    }
}