<script lang="ts">
    import { mobileMenuOpen } from '$lib/stores';

    function toggleMenu() {
        mobileMenuOpen.update(value => !value);
    }
</script>

<button
    type="button"
    class="md:hidden inline-flex items-center justify-center p-2 rounded-md text-gray-700 hover:text-gray-900 hover:bg-gray-100 focus:outline-hidden focus:ring-2 focus:ring-inset focus:ring-blue-500 dark:text-gray-200 dark:hover:text-white dark:hover:bg-gray-700"
    aria-controls="mobile-menu"
    aria-expanded={$mobileMenuOpen}
    onclick={toggleMenu}
>
    <span class="sr-only">Open main menu</span>
    <!-- Icon when menu is closed -->
    <svg
        class="h-6 w-6 {$mobileMenuOpen ? 'hidden' : 'block'}"
        fill="none"
        viewBox="0 0 24 24"
        stroke="currentColor"
        aria-hidden="true"
    >
        <path
            stroke-linecap="round"
            stroke-linejoin="round"
            stroke-width="2"
            d="M4 6h16M4 12h16M4 18h16"
        />
    </svg>
    <!-- Icon when menu is open -->
    <svg
        class="h-6 w-6 {$mobileMenuOpen ? 'block' : 'hidden'}"
        fill="none"
        viewBox="0 0 24 24"
        stroke="currentColor"
        aria-hidden="true"
    >
        <path
            stroke-linecap="round"
            stroke-linejoin="round"
            stroke-width="2"
            d="M6 18L18 6M6 6l12 12"
        />
    </svg>
</button>
