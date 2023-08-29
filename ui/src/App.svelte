<script>
    import Router, { push } from "svelte-spa-router";
    import Sidebar from "./components/Sidebar.svelte";
    import routes from "./routes";

    function routeLoaded(event) {
        if (event.detail.route == "/") push("/settings");
    }
</script>

<div>
    <div class="flex">
        <nav>
            <Sidebar />
        </nav>
        <main class="max-w-7xl mx-auto py-8 sm:py-10 md:py-12 sm:px-10 lg:px-12">
            <Router {routes} on:routeLoaded={routeLoaded} />
        </main>
    </div>
</div>
