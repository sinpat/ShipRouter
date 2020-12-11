<template>
  <v-dialog :value="errorText" width="400" persistent>
    <v-card>
      <v-card-title class="error" primary-title>
        <v-icon class="mr-2">mdi-alert-circle</v-icon>Error
      </v-card-title>
      <v-card-text class="mt-6 mb-2">
        <h2 class="mb-2">{{ errorText }}</h2>
        <span class="caption">{{ error }}</span>
      </v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn v-if="hasRetryFunction" @click="tryAgain">
          <v-icon left>mdi-replay</v-icon>Retry
        </v-btn>
        <v-btn @click="dismiss">
          <v-icon left>mdi-close</v-icon>Close
        </v-btn>
        <v-spacer></v-spacer>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script lang="ts">
import Vue from 'vue';
import Component from 'vue-class-component';

import ErrorState from '@/store/modules/error';

@Component({
  name: 'ErrorDialogComponent',
})
export default class ErrorDialog extends Vue {
  get errorText() {
    return ErrorState.text;
  }

  get error() {
    return ErrorState.error;
  }

  get hasRetryFunction() {
    return ErrorState.hasRetryFunction;
  }

  private tryAgain() {
    ErrorState.tryAgain();
  }

  private dismiss() {
    ErrorState.reset();
  }
}
</script>
