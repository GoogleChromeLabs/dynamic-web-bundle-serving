import { createServer } from 'http';
import { readFile } from 'fs';
import { analyze } from './index.js';
import { writeFileSync } from 'fs';

var args = process.argv.slice(2);
readFile(args[0], 'utf8', function (err,data) {
  if (err) {
    return console.log(err);
  }
  const result = analyze(data);
  writeFileSync(args[1], JSON.stringify(result, 0, 2));
});
