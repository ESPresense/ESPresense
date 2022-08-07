<script>
  import Router from "svelte-spa-router";
  import Sidebar from "./components/Sidebar.svelte";
  import routes from "./routes";
</script>

<div>
  <div class="flex">
    <nav>
      <Sidebar />
    </nav>
    <main class="max-w-7xl mx-auto py-8 sm:py-10 md:py-12 sm:px-10 lg:px-12">
      <Router {routes} />
    </main>
  </div>
</div>
