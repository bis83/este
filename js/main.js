// Copyright (c) bis83. Distributed under the MIT License.

class Application {
    constructor() {
        this.canvas = document.createElement("canvas");
        document.body.appendChild(this.canvas);
        this.gl = this.canvas.getContext("webgl2");
        this.fx = {
            sprite: new FxSprite(this.gl)
        };
        this.input = new Input();
        this.scene = new Scene();
        this.scene.onload(this.input, this.fx);
    }

    draw() {
        const width = window.innerWidth;
        if(width !== this.gl.canvas.width) {
            this.gl.canvas.width = width;
        }
        const height = window.innerHeight;
        if(height !== this.gl.canvas.height) {
            this.gl.canvas.height = height;
        }

        this.gl.viewport(0, 0, this.gl.canvas.width, this.gl.canvas.height);
        this.gl.clearColor(0, 0, 0, 0);
        this.gl.clearDepth(1.0);
        this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);

        const w = 800;
        const h = 600;
        this.gl.viewport((this.gl.canvas.width-w)/2, (this.gl.canvas.height-h)/2, w, h);
        this.fx.sprite.ortho(0, w, 0, h);

        this.scene.draw(this.fx);
    }

    loop() {
        requestAnimationFrame(() => this.loop());
        this.scene.update(this.input);
        this.draw();
    }
}

window.onload = () => {
    let app = new Application();
    app.loop();
};