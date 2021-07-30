
// Array and push value in front move all other to the back.
void pushToStack(int data[], int value) {
  for (int i = RPM_STACK_SIZE - 1; i >= 1; i--) {
    data[i] = data[i - 1];
  }
  data[0] = value;
}
