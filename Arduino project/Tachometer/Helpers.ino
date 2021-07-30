int getMax(int* array, int size) {
  unsigned int max = 0;
  for (int i = 0; i < size; i++) {
    int value = array[i];
    if (max < value) {
      max = value;
    }
  }
  return max;
}
