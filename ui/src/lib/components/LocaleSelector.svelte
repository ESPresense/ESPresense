<script>
  import { createEventDispatcher } from 'svelte'
  import { storeLocale } from '$lib/stores';

  export let value
  const dispatch = createEventDispatcher()
  function handleLocaleChange(event) {
    event.preventDefault()
    dispatch('locale-changed', event.target.value)
    storeLocale(event.target.value)
  }
</script>

<div class="locale-selector">
  <div class="select">
    <select {value} on:change={handleLocaleChange}>
      <option value="en">English</option>
      <option value="zh-cn">簡体中文</option>
      <option value="zh-tw">正體中文</option>
    </select>
  </div>
</div>
