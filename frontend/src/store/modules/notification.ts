import {
  Module,
  VuexModule,
  Mutation,
  getModule,
} from 'vuex-module-decorators';

import store from '../store';

@Module({
  dynamic: true,
  name: 'notificationState',
  store,
})
class ErrorState extends VuexModule {
  public message: string = '';

  @Mutation
  public setMessage(msg: string) {
    this.message = msg;
  }
}

export default getModule(ErrorState);
