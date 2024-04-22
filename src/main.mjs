import _ from '../src/vexflow_wrap.mjs';

cpp_draw_character(0xe050, 0, 100);

export async function main() {

    cpp_import_script('../external/vexflow-debug.js.js');

    const VexFlow = window.VexFlow;

    const { Factory } = VexFlow;
    const vf = new Factory({
      renderer: { elementId: 'output', width: 500, height: 200 },
    });
    
    const score = vf.EasyScore();
    const system = vf.System();
    
    system
      .addStave({
        voices: [
          score.voice(score.notes('C#5/q, B4, A4, G#4', { stem: 'up' })),
          score.voice(score.notes('C#4/h, C#4', { stem: 'down' })),
        ],
      })
      .addClef('treble')
      .addTimeSignature('4/4');
    
    vf.draw();
}

await main();