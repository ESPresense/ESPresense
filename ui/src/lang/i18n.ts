import { locale, init, _, getLocaleFromNavigator, register } from 'svelte-i18n'

import en from './locales/en.json'
import zh_tw from './locales/zh_tw.json'
import zh_cn from './locales/zh_cn.json'

register('en', async () => en)
register('zh-cn', async () => zh_cn)
register('zh-tw', async () => zh_tw)

/**
* Initializes the internationalization system
* @param {Object} options - Configuration options
* @param {string} options.withLocale - The initial locale to use
*/
function setupI18n(options) {
  const { withLocale: locale_ } = options

  // Initialize svelte-i18n
  init({
    initialLocale: locale_,
    fallbackLocale: 'en',
  })
}
const fixedLocales = {
  zh: 'zh-tw',
}
function getLocale() {
  // The value of locale is taken from the browser navigator.language, and the first part is taken here to adapt it to the language key defined by carpe.
  // where zh-TW requires special handling
  // https://developer.mozilla.org/en-US/docs/Web/API/Navigator/language
  let locale = getLocaleFromNavigator()
  if (locale) {
    locale = locale.toLocaleLowerCase().split('-')[0]
  } else {
    return 'en' // Default to English if browser locale is not available
  }

  return fixedLocales[locale] || locale
}
// We expose the svelte-i18n _ store so that our app has
// a single API for i18n
export { _, setupI18n, getLocale, locale }