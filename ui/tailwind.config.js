/** @type {import('tailwindcss').Config} */
export default {
  content: [
    './src/**/*.{html,js,svelte,ts}',
    './.svelte-kit/**/*.{html,js,svelte,ts}',
    './node_modules/svelte-table/**/*.{html,js,svelte,ts}'
  ],
  theme: {
    extend: {},
  },
  plugins: [],
  darkMode: 'class'
};
