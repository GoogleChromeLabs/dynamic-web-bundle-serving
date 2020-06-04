  /** @license
 * Copyright 2020 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
