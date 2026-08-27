<script lang="ts">
    import { themeMode } from '$lib/stores';

    const options = [
        {
            value: 'system',
            label: 'System',
            svg: `<rect width="20" height="14" x="2" y="3" rx="2"></rect>
                  <line x1="8" x2="16" y1="21" y2="21"></line>
                  <line x1="12" x2="12" y1="17" y2="21"></line>`
        },
        {
            value: 'light',
            label: 'Light',
            svg: `<circle cx="12" cy="12" r="4"></circle>
                  <path d="M12 2v2"></path>
                  <path d="M12 20v2"></path>
                  <path d="m4.93 4.93 1.41 1.41"></path>
                  <path d="m17.66 17.66 1.41 1.41"></path>
                  <path d="M2 12h2"></path>
                  <path d="M20 12h2"></path>
                  <path d="m6.34 17.66-1.41 1.41"></path>
                  <path d="m19.07 4.93-1.41 1.41"></path>`
        },
        {
            value: 'dark',
            label: 'Dark',
            svg: `<path d="M20.985 12.486a9 9 0 1 1-9.473-9.472c.405-.022.617.46.402.803a6 6 0 0 0 8.268 8.268c.344-.215.825-.004.803.401"></path>`
        }
    ] as const;

    $: themeIndex = options.findIndex((option) => option.value === $themeMode);
</script>

<div class="theme-toggle" style={`--theme-index: ${themeIndex};`}>
    <div class="theme-toggle__track" role="group" aria-label="Theme">
        <span class="theme-toggle__indicator"></span>
    {#each options as option}
        <button
            type="button"
            class={`theme-toggle__button ${$themeMode === option.value ? 'active' : ''}`}
            onclick={() => ($themeMode = option.value)}
            aria-pressed={$themeMode === option.value}
            aria-label={`${option.label} theme`}
            title={option.label}
        >
            <svg
                class="theme-toggle__icon"
                viewBox="0 0 24 24"
                aria-hidden="true"
                fill="none"
                stroke="currentColor"
                stroke-width="2"
                stroke-linecap="round"
                stroke-linejoin="round"
            >
                {@html option.svg}
            </svg>
        </button>
    {/each}
    </div>
</div>
