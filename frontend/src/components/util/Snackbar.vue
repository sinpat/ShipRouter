<template>
  <div>
    <v-snackbar
      @input="resetNotification"
      :value="notification"
      :timeout="timeout"
      bottom
      right
    >{{ notification }}</v-snackbar>
  </div>
</template>

<script lang="ts">
import Vue from 'vue';
import Component from 'vue-class-component';

import NotificationState from '@/store/modules/notification';

@Component({
  name: 'SnackbarComponent',
})
export default class Snackbar extends Vue {
  private timeout: number = 3000;

  get notification() {
    return NotificationState.message;
  }

  private resetNotification(value: boolean) {
    if (!value) {
      NotificationState.setMessage('');
    }
  }
}
</script>