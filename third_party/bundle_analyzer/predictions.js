import moduleClues from "./module-clues.js";

/**
 * Uses the list of module clues to find prediction matches for a minified webpack module,
 * provided as a string
 *
 * @param {string} functionText - A function extracted from a webpack bundle
 */
export function predictModuleName(functionText) {
  // Note, this is obviously a potential performance problem--as we're currently doing a
  // full search of the (potententially very large) bundle for every clue in the list.
  // Running this in a worker seems to solve the problem for now, but if the clue list got
  // very long, a different solution might be required.
  let matchedClue = moduleClues.find(clue =>
    typeof clue.includes === "object" // Handle regex clues differently from strings
      ? clue.includes.test(functionText)
      : functionText.includes(clue.includes)
  );
  if (!matchedClue) {
    matchedClue = { module: "", url: "", includes: "" };
  }
  return matchedClue;
}

