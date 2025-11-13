addToLibrary({
  requestSize: function(w, h) {
    let canvas = document.getElementById("canvas").parentElement;
    canvas.width = w;
    canvas.height = h;

    console.log(w);
    console.log(h);

    let message = { width: w, height: h };
    parent.postMessage(message);
  },
});
