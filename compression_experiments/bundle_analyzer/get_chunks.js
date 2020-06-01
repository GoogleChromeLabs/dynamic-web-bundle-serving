import { createServer } from 'http';
import { readFile } from 'fs';
import { analyze } from './index.js';
import { writeFileSync } from 'fs';

readFile('/Users/elkir/Documents/bundle/bundle_analyzer/text_bundle.txt', 'utf8', function (err,data) {
  if (err) {
    return console.log(err);
  }
  const result = analyze(data);
  writeFileSync('parsed_bundle.json', JSON.stringify(result, 0, 2));
});
