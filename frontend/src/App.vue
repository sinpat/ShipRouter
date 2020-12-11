<template>
  <div id="app">
    <v-app>
      <!-- Toolbar -->
      <v-app-bar app clipped-left>
        <v-toolbar-title class="headline text-uppercase">
          <span>Ship Routing</span>
        </v-toolbar-title>
        <v-spacer></v-spacer>
      </v-app-bar>

      <!-- Main content -->
      <v-main class="mx-2">
        <v-container>
          <Home />
        </v-container>
      </v-main>

      <!-- Footer -->
      <v-footer>
        <span>&copy; Lukas Epple &amp; Patrick Singer 2020</span>
      </v-footer>
      <ErrorDialog />
      <Snackbar />
    </v-app>
  </div>
</template>

<script lang="ts">
import Vue from 'vue';
import Component from 'vue-class-component';

import ErrorDialog from '@/components/util/ErrorDialog.vue';
import Snackbar from '@/components/util/Snackbar.vue';
import Home from '@/views/Home.vue';

@Component({
  name: 'App',
  components: { Home, ErrorDialog, Snackbar },
})
export default class App extends Vue {}
</script>

<style lang="scss">
#app {
  font-family: 'Roboto';
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
}
</style>
