/**
 * A server that lets us demo the functionality of analyze().
 */
import { createServer } from 'http';
import { analyze } from './index.js';

createServer(async (req, res) => {
  switch (req.url) {
    // POST /analyze⏎⏎<bundle> - analyze the bundle and return JSON
    case '/analyze': {
      let body = '';
      for await (const chunk of req) body += chunk;
      const result = analyze(body);
      res.setHeader('content-type', 'application/json');
      res.end(JSON.stringify(result, 0, 2));
      break;
    }

    // GET / - serves a web page
    default: {
      res.setHeader('content-type', 'text/html');
      res.end(`
        <header>
          <button id=analyze>Analyze</button>
        </header>
        <textarea id=bundle placeholder="Paste bundle code here..."></textarea>
        <textarea id=output placeholder="(output)" readonly></textarea>
        <script>
          analyze.onclick = async () => {
            const res = await fetch('/analyze', { method:'POST', body: bundle.value });
            const info = await res.json();
            output.value = JSON.stringify(info, 0, 2);
          };
        </script>
        <style>
          html,body{margin:0;padding:0;display:flex;flex-direction:column;flex:1;}
          textarea{flex:1;margin:10px;padding:10px;font:12px monospace;white-space:pre;}
          header{background:#abc;padding:10px;}
        </style>
      `);
      break;
    }
  }
}).listen(process.env.PORT);
