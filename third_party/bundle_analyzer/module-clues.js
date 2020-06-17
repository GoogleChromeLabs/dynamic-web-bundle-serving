// Exports an array of "clue" objects for use in automatically deducing the source of
// minified code in webpack bundles

export default [
  {
    module: "bluebird",
    includes: "prototype.disableTrampolineIfNecessary",
    url: "https://github.com/petkaantonov/bluebird",
    polyfill: true
  },
  {
    module: "bn",
    includes: /67108864\s?&\s?67108863/,
    url: "https://github.com/indutny/bn.js/"
  },
  {
    module: "character-entities",
    includes: "CapitalDifferentialD:",
    url: "https://www.npmjs.com/package/character-entities"
  },
  {
    module: "corejs/promise",
    includes: "Promise can't be resolved itself",
    url:
      "https://github.com/zloirock/core-js/blob/master/packages/core-js/modules/es.promise.js",
    polyfill: true
  },
  {
    module: "elliptic",
    includes: "prototype._getEndoRoots",
    url: "https://github.com/indutny/elliptic"
  },
  {
    module: "emotion",
    includes: 'querySelectorAll("style[data-emotion-"',
    url: "https://github.com/emotion-js/emotion"
  },
  {
    module: "fingerprint2",
    includes: "getWebglVendorAndRenderer",
    url: "https://github.com/Valve/fingerprintjs2/"
  },
  {
    module: "i18next",
    includes: "options.overloadTranslationOptionHandler",
    url: "https://www.i18next.com/"
  },
  {
    module: "immutable-js",
    includes: "Expected Array or iterable object of values, or keyed object",
    url: "https://github.com/immutable-js/immutable-js"
  },
  {
    module: "lodash",
    includes: "__lodash_placeholder__",
    url: "https://lodash.com/"
  },
  {
    module: "marked",
    includes: 'Error("Infinite loop on byte: "',
    url: "https://github.com/markedjs/marked"
  },
  {
    module: "mobile-detect",
    includes: "PrestigioTablet",
    url: "http://hgoebl.github.io/mobile-detect.js/"
  },
  {
    module: "moment",
    includes: "localeData().monthsShort",
    url: "https://momentjs.com/"
  },
  {
    module: "next/route",
    includes: 'Error("Cannot update unavailable route:',
    url: ""
  },
  {
    module: "next/client/index",
    includes: 'emit("before-reactdom-render"',
    url:
      "https://github.com/zeit/next.js/blob/canary/packages/next/client/index.js"
  },
  {
    module: "node/buffer",
    includes:
      'Error("If encoding is specified then the first argument must be a string")',
    url: ""
  },
  {
    module: "node/url",
    includes: "\"Parameter 'url' must be a string, not \"",
    url: "https://github.com/nodejs/node/blob/master/lib/url.js"
  },
  {
    module: "nprogress",
    includes: "nprogress-custom-parent",
    url: "https://github.com/rstacruz/nprogress"
  },
  {
    module: "popmotion",
    includes: "are of different format, or a value might have changed value",
    url: "https://popmotion.io/"
  },
  {
    module: "preact",
    includes: "__preactattr_",
    url: "https://preactjs.com/"
  },
  {
    module: "raven-js",
    includes: "Error: Raven has already been configured",
    url: "https://www.npmjs.com/package/raven-js"
  },
  {
    module: "react-aria-modal",
    includes: "react-aria-modal instances should have",
    url: "https://github.com/davidtheclark/react-aria-modal"
  },
  {
    module: "react-i18Next",
    includes: "getI18nTranslate.bind",
    url: "https://react.i18next.com/"
  },
  {
    module: "react-helmet",
    includes: "convertReactPropstoHtmlAttributes",
    url: "https://github.com/nfl/react-helmet"
  },
  {
    module: "react-intl",
    includes: "pluralRuleFunction:function",
    url: "https://github.com/formatjs/react-intl"
  },
  {
    module: "react-redux",
    includes: "parentSub.addNestedSub",
    url: "https://react-redux.js.org/"
  },
  {
    module: "regenerator-runtime",
    includes: 'Error("Generator is already running")',
    url: "https://www.npmjs.com/package/regenerator-runtime",
    polyfill: true
  },
  {
    module: "styled-jsx/stylesheet",
    includes: "StyleSheet: illegal rule:",
    url: "https://github.com/zeit/styled-jsx"
  },
  {
    module: "whatwg-fetch",
    includes: 'searchParams:"URLSearchParams"',
    url: "https://www.npmjs.com/package/whatwg-fetch",
    polyfill: true
  },
  // VAGUE SELECTORS BELOW
  {
    module: "possible application code",
    includes: "this hasn't been initialised - super() hasn't been called", //Babel adds this during transpilation
    url: ""
  },
  {
    module: "react module",
    includes: "https://reactjs.org/docs/error-decoder.html?invariant="
  }
];

// Snippet for authoring clues:
// "Module_Clue": {
//     "prefix": "clue",
//     "body": [
//         "{",
//         "\tmodule: '$1',",
//         "\tincludes: '$2',",
//         "\turl: '$3'",
//         "},"
//     ],
//     "description": "Clue syntax for bundle analyzer"
// }

