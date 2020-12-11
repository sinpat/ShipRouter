import Vue from 'vue';
import Vuetify from 'vuetify/lib';
import App from './App.vue';
import store from './store/store';

Vue.use(Vuetify);

Vue.config.productionTip = false;

new Vue({
  store,
  vuetify: new Vuetify({
    theme: {
      // dark: new Date().getHours() >= 21 || new Date().getHours() <= 7,
    },
    icons: {
      iconfont: 'mdi', // Default - only for display purposes
    },
  }),
  render: h => h(App),
}).$mount('#app');
